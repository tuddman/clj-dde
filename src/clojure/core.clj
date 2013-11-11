(ns clj-dde.core
  (:import
            [com.pretty_tools.dde.client DDEClientConversation DDEClientEventListener]))


(System/getProperty "java.library.path")

(defn fooas-fn
  []
  (str main.java.Foo/value))

(defn foo
  "I don't do much, except..."
  []
  (println (main.java.Foo/value)))

(defn get-a-conversation
  []
  (str (com.pretty_tools.dde.client.DDEClientConversation/connect)))


(fooas-fn)

(foo)

(range 5
       (print "yoyo"))
