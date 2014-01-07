(ns clj-dde.core
  [:import
        [com.pretty_tools.dde.client DDEClientConversation]
        [com.pretty_tools.dde.client DDEClientEventListener]
        [com.pretty_tools.dde ClipboardFormat]
        [com.pretty_tools.dde DDEException]
        [com.pretty_tools.dde DDEMLException]])

#_(System/getProperty "java.library.path")



;; implement the DDEClientEventListener interface

(defn setEventListener
  [conv]
(.setEventListener conv
  (reify DDEClientEventListener
  (onItemChanged [this topic item data]
               (prn (str "Item Changed: " topic  " , " item  " , " data )))
  (onDisconnect [this]
                (prn (str "onDisconnect() called."))))))


(defn listen
  [conv]
(.setEventListener conv
  (reify DDEClientEventListener
  (onItemChanged [this _ _ data]
               (data))
  (onDisconnect [this]
                (prn (str "onDisconnect() called."))))))


(defn listen-and-print-data
  [conv]
(.setEventListener conv
  (reify DDEClientEventListener
  (onItemChanged [this _ _ data]
               (prn (str data)))
  (onDisconnect [this]
                (prn (str "onDisconnect() called."))))))


;; instantiate a DDEClientConversation

(defn conversation
  []
  (DDEClientConversation.))


;; Method Definitions for a DDEClientConversation.  These are the 'direct' translations of java->clojure.
;; TODO: define more 'idiomatic, syntactic sugar' usage methods.

(defn setTimeout
  [conv ms]
  (.setTimeout conv ms))

(defn getTimeout
  "returns timeout length in ms"
  [conv]
  (.getTimeout conv))

(defn connect
  [conv app topic]
  (.connect conv app topic))

(defn disconnect
  [conv]
  (.disconnect conv))

(defn startAdvice
  [conv item]
  (.startAdvice conv item))

(defn stopAdvice
  [conv item]
  (.stopAdvice conv item))

(defn poke
  [conv topic input]
  (.poke conv topic input))

(defn request
  [conv item]
  (.request conv item))

(defn execute
  [conv command]
  (.execute conv command))

(defn getService
  [conv]
  (.getService conv))

(defn getTopic
  [conv]
  (.getTopic conv))


