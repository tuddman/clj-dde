# clj-dde

A Clojure library (really a wrapper around the Java DDE library provided by pretty_tools) to interoperate with DDE (dynamic data exchange) sources.

## Rationale

1. Sometimes you need to connect to a DDE source, and
2. would rather do it in Clojure > some other language


## Setup

Make sure you have [Leiningen](http://leiningen.org/) installed.

Your `project.clj` should include something like the following:

```clj
(defproject foo "0.1.0"
  ...
  :dependencies [[org.clojure/clojure "1.5.1"]
                 [clj-dde "0.1.1"]]
  ...)
```

## Usage

Work in Progress.

When you need to connect with a DDE data source...


`(dde/convesation)` to setup a connection.
`(dde/connect "excel" "sheet1")` to connect to a datasource (excel)
`(dde/request excel-conv "R1C1")` to 'poll' the data contained in cell R1C1

check out the dde-example folder for further examples and usage.

## Issues

If you get the message "java.lang.UnsatisfiedLinkError: no JavaDDEx64 (or JavaDDE) in java.library.path"  you need to add the javaDDEx64.dll (or javaDDE.dll) file (it's in /resources) to your classpath directory:

`(System/getProperty "java.library.path")` to find out where this file needs to go.

## License

Copyright © 2013 tuddman

Distributed under the Eclipse Public License; the same as Clojure.


The various included Java files (pretty_tools library) are licensed under their respective licenses.
