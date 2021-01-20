// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Based on this example:
// https://github.com/google/sandboxed-api/blob/master/sandboxed_api/sandbox2/examples/static/static_sandbox.cc

#include "sandboxed_api/sandbox2/executor.h"

#include <fcntl.h>
#include <glog/logging.h>
#include <linux/socket.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <syscall.h>
#include <unistd.h>

#include <csignal>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "absl/base/internal/raw_logging.h"
#include "absl/memory/memory.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_format.h"
#include "sandboxed_api/sandbox2/limits.h"
#include "sandboxed_api/sandbox2/policy.h"
#include "sandboxed_api/sandbox2/policybuilder.h"
#include "sandboxed_api/sandbox2/result.h"
#include "sandboxed_api/sandbox2/sandbox2.h"
#include "sandboxed_api/sandbox2/util.h"
#include "sandboxed_api/sandbox2/util/bpf_helper.h"
#include "sandboxed_api/util/flag.h"
#include "sandboxed_api/util/runfiles.h"
#include "tools/cpp/runfiles/runfiles.h"

std::unique_ptr<sandbox2::Policy> GetPolicy(std::string nodePath) {
  return sandbox2::PolicyBuilder()

      // Libraries for binaries are required to run a binary.
      .AddLibrariesForBinary(nodePath)

      // System folders and files.
      .AddFile("/dev/urandom", false)
      .AddTmpfs("/dev/shm", 256 << 20)  // 256MB
      .AddFile("/etc/localtime")
      // Proc needed for retrieving fd.
      .AddDirectory("/proc", false)
      .AddFile("/dev/null", false)
      .AddFile("/etc/ld.so.cache")

      // File descriptor use.
      .AllowRead()
      .AllowReaddir()
      .BlockSyscallWithErrno(__NR_ioctl, ENOTTY)

      // Misc.
      .AllowDynamicStartup()
      .AllowSyscall(__NR_getpid)
      .AllowSyscalls({
          __NR_access,   // GRTE/v5
          __NR_getpid,   // GRTE/v5
          __NR_mkdir,    //
          __NR_sysinfo,  // GRTE/v5
          __NR_unlink,   //
      })

      // Temporary, for development.
      .AddFile("/usr/local/google/home/eliaskassell/tmp.js")

      // Aggressively fail if policy builder fails.
      .BuildOrDie();
}

void OutputFD(int stdoutFd, int errFd) {
  for (;;) {
    char stdoutBuf[4096];
    char stderrBuf[4096];
    ssize_t stdoutRLen = read(stdoutFd, stdoutBuf, sizeof(stdoutBuf));
    ssize_t stderrRLen = read(errFd, stderrBuf, sizeof(stderrBuf));
    printf("stdout: '%s'\n", std::string(stdoutBuf, stdoutRLen).c_str());
    printf("stderr: '%s'\n", std::string(stderrBuf, stderrRLen).c_str());
    if (stdoutRLen < 1) {
      break;
    }
  }
}

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InitGoogleLogging(argv[0]);

  std::string workspaceFolder = "df/";

  std::string nodeRelativePath(argv[1]);
  std::string nodePath =
      sapi::GetDataDependencyFilePath(workspaceFolder + nodeRelativePath);

  std::string compileRelativePath(argv[2]);
  std::string compilePath =
      sapi::GetDataDependencyFilePath(workspaceFolder + compileRelativePath);

  std::vector<std::string> args = {
      nodePath,
      // compilePath,
      "/usr/local/google/home/eliaskassell/tmp.js",
  };
  printf("Running command: '%s'\n", (args[0] + " " + args[1]).c_str());
  auto executor = absl::make_unique<sandbox2::Executor>(nodePath, args);

  executor->set_enable_sandbox_before_exec(true)
      .limits()
      // Remove restrictions on the size of address-space of sandboxed
      // processes.
      ->set_rlimit_as(RLIM64_INFINITY);

  auto stdoutFd = executor->ipc()->ReceiveFd(STDOUT_FILENO);
  printf("stdoutFd: %i\n", stdoutFd);
  auto stderrFd = executor->ipc()->ReceiveFd(STDERR_FILENO);
  printf("stderrFd: %i\n", stderrFd);

  auto policy = GetPolicy(nodePath);
  sandbox2::Sandbox2 s2(std::move(executor), std::move(policy));
  printf("Policy applied, running\n");

  sandbox2::Result result;
  if (s2.RunAsync()) {
    OutputFD(stdoutFd, stderrFd);
    result = s2.AwaitResult();
  } else {
    printf("Sandbox failed\n");
  }
  printf("Run complete\n");

  printf("Final execution status: %s\n", result.ToString().c_str());

  return result.final_status() == sandbox2::Result::OK ? EXIT_SUCCESS
                                                       : EXIT_FAILURE;
}
