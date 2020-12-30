# hts_engine_API


![C/C++ CI](https://github.com/r9y9/hts_engine_API/workflows/C/C++%20CI/badge.svg)
[![Build Status](https://travis-ci.org/r9y9/hts_engine_API.svg?branch=master)](https://travis-ci.org/r9y9/hts_engine_API)
[![Build status](https://ci.appveyor.com/api/projects/status/7tm96g50a9i43mhl/branch/master?svg=true)](https://ci.appveyor.com/project/r9y9/hts-engine-api/branch/master)

A fork of hts_engine_API

## Why

Wanted to fork it with *git*.

**NOTE**: To preserve history of cvs version of hts_engine_API, this fork was originially created by:

```
git cvsimport -v \
  -d :pserver:anonymous@hts-engine.cvs.sourceforge.net:/cvsroot/hts-engine \
  -C hts_engine_API hts_engine_API
```

## Supported platforms

- Linux
- Mac OS X
- Windows (gcc/msvc)

## Changes

The important changes from the original hts_engine_API are summarized below:

- CMake support
- Add pkg-config suppport
- Continuous integratioin support
- keep sementic versioning http://semver.org/
