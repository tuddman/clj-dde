(ns clj-dde.core
  (:import [com.pretty_tools.dde.client DDEClientConversation
                                        DDEClientEventListener]
           [com.pretty_tools.dde ClipboardFormat
                                 DDEException
                                 DDEMLException]))

;; implement the DDEClientEventListener interface

(defn set-event-listener
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

(defn set-timeout
  [conv ms]
  (.setTimeout conv ms))

(defn get-timeout
  "returns timeout length in ms"
  [conv]
  (.getTimeout conv))

(defn connect
  [conv app topic]
  (.connect conv app topic))

(defn disconnect
  [conv]
  (.disconnect conv))

(defn start-advice
  [conv item]
  (.startAdvice conv item))

(defn stop-advice
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

(defn get-service
  [conv]
  (.getService conv))

(defn get-topic
  [conv]
  (.getTopic conv))
