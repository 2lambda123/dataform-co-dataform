import * as protos from "@dataform/protos";
import { Dataform } from "./index";

export type OContextable<T> = T | ((ctx: OperationContext) => T);

export class Operation {
  proto: protos.IOperation = protos.Operation.create();

  // Hold a reference to the Dataform instance.
  dataform: Dataform;

  // We delay contextification until the final compile step, so hold these here for now.
  private contextableStatements: OContextable<string | string[]>;

  public statement(statement: OContextable<string | string[]>) {
    this.contextableStatements = statement;
  }

  public dependency(value: string | string[]) {
    var newDependencies = typeof value === "string" ? [value] : value;
    newDependencies.forEach(d => {
      if (this.proto.dependencies.indexOf(d) < 0) {
        this.proto.dependencies.push(d);
      }
    });
    return this;
  }

  compile() {
    var context = new OperationContext(this);

    var appliedStatements = context.apply(this.contextableStatements);
    this.proto.statements =
      typeof appliedStatements == "string"
        ? [appliedStatements]
        : appliedStatements;
    this.contextableStatements = null;

    return this.proto;
  }
}

export class OperationContext {
  private operation?: Operation;

  constructor(operation: Operation) {
    this.operation = operation;
  }

  public ref(name: string) {
    this.operation.dependency(name);
    return this.operation.dataform.ref(name);
  }

  public dependency(name: string | string[]) {
    this.operation.dependency(name);
    return "";
  }

  public apply<T>(value: OContextable<T>): T {
    if (typeof value === "function") {
      return (value as any)(this);
    } else {
      return value;
    }
  }
}
