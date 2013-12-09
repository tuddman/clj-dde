(ns clj-dde.core
;  (:gen-class)
  [:import
        [com.pretty_tools.dde.client DDEClientConversation]
        [com.pretty_tools.dde.client DDEClientEventListener]
        [com.pretty_tools.dde ClipboardFormat]
        [com.pretty_tools.dde DDEException]
        [com.pretty_tools.dde DDEMLException]
        [java.util Date]])

(System/getProperty "java.library.path")


;; instantiate a DDEClientConversation


(defn conversation
  []
  (DDEClientConversation.))

(defn setTimeout
  [conv ms]
  (.setTimeout conv ms))

(defn connect
  [conv app topic]
  (.connect conv app topic))

(defn startAdvice
  [conv]
  (.startAdvice conv))

(defn poke
  [conv input]
  (.poke conv input))


;; implement the interface

(defn ev-listener
  []
(reify DDEClientEventListener
  (onItemChanged [this topic item data]
               (str "onItemChanged: " topic  " , " item  " , " data " )"))
  (onDisconnect [this]
                (str "onDisconnect() called."))))




#_(defn -main
  [& args]
  (str "main got called."))







