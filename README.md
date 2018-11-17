# Development

## Requirements

- Node (8+)
- NPM
- [Jekyll](https://jekyllrb.com/docs/installation/)

## Setup

Install node dev packages:

```bash
npm i
```

Install lerna managed packages:

```bash
npx lerna bootstrap
```

## Build

Compile typescript for all packages:

```bash
./scripts/build/cli
```

## Run

Runs the command line app:

```bash
./scripts/run/cli --help
```

## Compile example project

Outputs a compiled JSON graph for one of the example projects:

```bash
./scripts/run/cli compile ts/example-bigquery
```

## Serve docs

Runs Jekyll locally at http://localhost:4000

```bash
(cd docs && bundle exec jekyll serve)
```

## Run tests

Run mocha tests in `ts/tests`:

```bash
./scripts/test
```

## Publish

Publish a new version with lerna:

```bash
npx lerna publish
```
