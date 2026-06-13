# Cater

## Introduction
Cater is a compiled language designed for JVM and Android platforms. It transpiles code to standard Java and builds using official toolchains. No bulky runtime is needed, delivering stable performance and compact output files.

## Build Workflow
### Standard Build (Output JAR)
`.ct` Source → Java → Class files → JAR

### Android Build (Output DEX)
Append the `--dex` flag to generate Android DEX files:
`.ct` Source → Java → Class files → JAR → DEX (via d8/dx)

## Usage
### Compile to JAR
```bash
caterc main.ct
 
 
Compile to Android DEX
 
```bash
  

caterc main.ct --dex
```
 
 
Features
 
- Full compatibility with Java libraries and Android SDK
- Stable touch and event response on Android
- Lightweight, no redundant dependencies
- Built on mature standard toolchains
 
File Extension
 
Source file: .ct
