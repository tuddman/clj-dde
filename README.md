# clj-dde

A Clojure library (really a wrapper around the Java DDE library provided by pretty_tools) to interoperate with DDE (dynamic data exchange) sources.

## Usage

When you need to connect with a DDE data source...



If you get the message "java.lang.UnsatisfiedLinkError: no JavaDDEx64 in java.library.path"  you need to add the javaDDE64.dll file (in /resources) to your classpath:

(System/getProperty "java.library.path") to find out

## License

Copyright © 2013 tuddman

Distributed under the Eclipse Public License; the same as Clojure.


The various included Java files (pretty_tools library) are licensed under their respective licences.
