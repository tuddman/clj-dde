(ns clj-dde.core
  (:gen-class)
  [:import
        [com.pretty_tools.dde.client DDEClientConversation]
        [com.pretty_tools.dde.client DDEClientEventListener]
        [com.pretty_tools.dde ClipboardFormat]
        [com.pretty_tools.dde DDEException]
        [com.pretty_tools.dde DDEMLException]])

(System/getProperty "java.library.path")


;; instantiate a DDEClientConversation

;; MetaTrader --

(def mt4-conv-bid (DDEClientConversation.))
(def mt4-conv-ask (DDEClientConversation.))

(.setTimeout mt4-conv-bid 3000)
(.setTimeout mt4-conv-ask 3000)

(.connect mt4-conv-bid "MT4" "BID")
(.connect mt4-conv-ask "MT4" "ASK")

(.startAdvice mt4-conv-bid "EURUSD")
(.startAdvice mt4-conv-ask "EURUSD")

; (.request mt4-conv-bid "EURUSD")
; (.request mt4-conv-ask "EURUSD")


(defn get-eu-bid
  []
 (Float. (.request mt4-conv-bid "EURUSD")))


(defn get-bid
  [ccy]
  (Float. (.request mt4-conv-bid #(clojure.string/upper-case ccy))))


(def eubid  (get-eu-bid))

(def eubid-now (atom 1.0))

eubid

@eubid-now

(compare-and-set! eubid-now @eubid-now (get-eu-bid))

@eubid-now


(defn stream-pair [ccy]
  ())


#_(while (not= @eubid-now 1.0 )
  (do
    (if-not (= @eubid-now (get-eu-bid) )
      (do
      (compare-and-set! eubid-now @eubid-now (get-eu-bid))
      (println @eubid-now)))))




; (.setEventListener mt4-link (DDEClientEventListener))

;    void onItemChanged(String topic, String item, String data);

#_(defn ELOnItemChanged
  []
(reify DDEClientEventListener
  (onItemChanged [this topic item data]
               (str "onItemChanged: " topic  " , " item  " , " data " )"))))

#_(defn ELOnDisconnect
  []
(reify DDEClientEventListener
  (onDisconnect [this]
               (str "onDisconnect called."))))

; (.setEventListener mt4-conv (ELOnItemChanged))

; (ELOnItemChanged)


(defn -main
  [& args]
  (str "main got called."))


























