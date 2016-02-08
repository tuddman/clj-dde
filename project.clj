(defproject clj-dde "0.2.0-dev"
  :description "clj-dde: for when you need to connect to a DDE (dynamic data exchange) source"
  :url "http://github.com/tuddman/clj-dde"
  :license {:name "Eclipse Public License"
            :url "http://www.eclipse.org/legal/epl-v10.html"}
  :java-source-paths ["src/java"]
  :profiles {:dev {:dependencies [[org.clojure/clojure "1.8.0"]]
                   :jvm-opts ["-Djava.library.path=resources"]}})
