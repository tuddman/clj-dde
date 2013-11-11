# clj-dde

A Clojure library (really a wrapper around the Java DDE library provided by pretty_tools) to interoperate with DDE (dynamic data exchange) sources.

## Rationale

1. Sometimes you need to connect to a DDE source, and
2. would rather do it in Clojure > some other language

## Usage

When you need to connect with a DDE data source...


## Setup

`lein javac` to compile the java files.

## Issues

If you get the message "java.lang.UnsatisfiedLinkError: no JavaDDEx64 (or JavaDDE) in java.library.path"  you need to add the javaDDE64.dll (or javaDDE.dll) file (it's in /resources) to your classpath directory:

`(System/getProperty "java.library.path")` to find out

## License

Copyright © 2013 tuddman

Distributed under the Eclipse Public License; the same as Clojure.


The various included Java files (pretty_tools library) are licensed under their respective licences.
