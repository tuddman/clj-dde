(ns clj-dde.core
  (:import [com.pretty_tools.dde.client DDEClientConversation
                                        DDEClientEventListener]
           [com.pretty_tools.dde ClipboardFormat
                                 DDEException
                                 DDEMLException]))

;; implement the DDEClientEventListener interface

(defn set-event-listener
  [conv & {:keys [on-disconnect on-item-changed]
           :or {on-disconnect (fn [_])
                on-item-changed (fn [_ _ _ _])}}]
  (doto conv
    (.setEventListener
      (reify DDEClientEventListener
        (onItemChanged [this topic item data] (on-item-changed this topic item data))
        (onDisconnect  [this] (on-disconnect this))))))

(defn listen
  [conv]
  (set-event-listener conv
                      :on-disconnect (fn [_] (prn (str "onDisconnect() called.")))))

(defn listen-and-print-data
  [conv]
  (set-event-listener conv
                      :on-item-changed (fn [_ _ _ data] (prn (str data)))
                      :on-disconnect   (fn [_] (prn (str "onDisconnect() called.")))))

;; instantiate a DDEClientConversation

(defn conversation
  []
  (DDEClientConversation.))


;; Method Definitions for a DDEClientConversation.  These are the 'direct' translations of java->clojure.
;; TODO: define more 'idiomatic, syntactic sugar' usage methods.

(defn set-timeout
  [conv ms]
  (doto conv (.setTimeout ms)))

(defn get-timeout
  "returns timeout length in ms"
  [conv]
  (.getTimeout conv))

(defn connect
  [conv app topic]
  (doto conv (.connect app topic)))

(defn disconnect
  [conv]
  (doto conv .disconnect))

(defn start-advice
  [conv item]
  (doto conv (.startAdvice item)))

(defn stop-advice
  [conv item]
  (doto conv (.stopAdvice item)))

(defn poke
  [conv topic input]
  (doto conv (.poke topic input)))

(defn request
  [conv item]
  (.request conv item))

(defn execute
  [conv command]
  (doto conv (.execute command)))

(defn get-service
  [conv]
  (.getService conv))

(defn get-topic
  [conv]
  (.getTopic conv))
