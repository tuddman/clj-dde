(ns clj-dde.core
  [:import
        [com.pretty_tools.dde.client DDEClientConversation]
        [com.pretty_tools.dde.client DDEClientEventListener]
        [com.pretty_tools.dde ClipboardFormat]
        [com.pretty_tools.dde DDEException]
        [com.pretty_tools.dde DDEMLException]])

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

(defn disconnect
  [conv]
  (.disconnect conv))

(defn startAdvice
  [conv]
  (.startAdvice conv))

(defn stopAdvice
  [conv]
  (.stopAdvice conv))

(defn poke
  [conv topic input]
  (.poke conv topic input))

(defn request
  [conv item]
  (.request conv item))


;; implement the DDEClientEventListener interface

(defn ev-listener
  []
(reify DDEClientEventListener
  (onItemChanged [this topic item data]
               (str "onItemChanged: " topic  " , " item  " , " data " )"))
  (onDisconnect [this]
                (str "onDisconnect() called."))))

