(ns clj-dde.metatrader4
;  (:gen-class)
  [:import
        [com.pretty_tools.dde.client DDEClientConversation]
        [com.pretty_tools.dde.client DDEClientEventListener]
        [com.pretty_tools.dde ClipboardFormat]
        [com.pretty_tools.dde DDEException]
        [com.pretty_tools.dde DDEMLException]
        [java.util Date]])


;; MetaTrader --

; *note* the 'Enable DDE Server' box must be checked in Metatrader -> Tools -> Options -> Server

(def pairs '("EURUSD" "GBPUSD" "EURGBP" "EURJPY" "GBPJPY" "USDJPY"))

(def mt4-conv-bid (DDEClientConversation.))
(def mt4-conv-ask (DDEClientConversation.))
(def mt4-conv-high (DDEClientConversation.))
(def mt4-conv-low (DDEClientConversation.))
(def mt4-conv-time (DDEClientConversation.))
(def mt4-conv-quote (DDEClientConversation.))

(.setTimeout mt4-conv-bid 3000)
(.setTimeout mt4-conv-ask 3000)
(.setTimeout mt4-conv-high 3000)
(.setTimeout mt4-conv-low 3000)
(.setTimeout mt4-conv-time 3000)
(.setTimeout mt4-conv-quote 3000)

(.connect mt4-conv-bid "MT4" "BID")
(.connect mt4-conv-ask "MT4" "ASK")
(.connect mt4-conv-high "MT4" "HIGH")
(.connect mt4-conv-low "MT4" "LOW")
(.connect mt4-conv-time "MT4" "TIME")
(.connect mt4-conv-quote "MT4" "QUOTE")



#_(def eu-high
(.request mt4-conv-high "EURUSD"))

(re-find #"\d+" eu-high)

#_(defn start-advising-on
  [pairs]
(doall
  (map #(.startAdvice mt4-conv-bid %) pairs)
  (map #(.startAdvice mt4-conv-ask %) pairs)
  (map #(.startAdvice mt4-conv-high %) pairs)
  (map #(.startAdvice mt4-conv-low %) pairs)
  (map #(.startAdvice mt4-conv-time %) pairs)
  (map #(.startAdvice mt4-conv-quote %) pairs)))

#_(start-advising-on pairs)


(map #(.startAdvice mt4-conv-bid %) pairs)
(map #(.startAdvice mt4-conv-ask %) pairs)
(map #(.startAdvice mt4-conv-high %) pairs)
(map #(.startAdvice mt4-conv-low %) pairs)
(map #(.startAdvice mt4-conv-time %) pairs)
(map #(.startAdvice mt4-conv-quote %) pairs)


(defn get-bid
  [ccy]
  (Float. (.request mt4-conv-bid  ccy)))

(defn get-ask
  [ccy]
  (Float. (.request mt4-conv-ask  ccy)))

(defn get-high
  [ccy]
  (Float. (.request mt4-conv-high  ccy)))

(defn get-low
  [ccy]
  (Float. (.request mt4-conv-low  ccy)))

(defn get-time
  [ccy]
  ((.request mt4-conv-time  ccy)))

(defn get-quote
  [ccy]
  ((.request mt4-conv-quote  ccy)))


(def eubid (get-bid "EURUSD"))
(def eubid-now (atom 1.0))
eubid
@eubid-now
(compare-and-set! eubid-now @eubid-now (get-bid "EURUSD"))
@eubid-now


(def eu-high (get-high "EURUSD"))
(def eu-low (get-low "EURUSD"))
#_(def eu-time (get-time "EURUSD"))
#_(def eu-quote (get-quote "EURUSD"))
eu-high
eu-low

(def gubid (get-bid "GBPUSD"))
(def gubid-now (atom 1.0))
gubid
@gubid-now
(compare-and-set! gubid-now @gubid-now (get-bid "GBPUSD"))
@gubid-now


(defn stream-pair [ccy]
  ())


(while (not= @eubid-now 1.0 )
  (do
    (if-not (= @eubid-now (get-bid "EURUSD") )
      (do
      (compare-and-set! eubid-now @eubid-now (get-bid "EURUSD"))
      (println @eubid-now "@" (Date.))))))


;; implement the DDEClientEventListener interface

(defn ev-listener
  []
(reify DDEClientEventListener
  (onItemChanged [this topic item data]
               (str "onItemChanged: " topic  " , " item  " , " data " )"))
  (onDisconnect [this]
                (str "onDisconnect() called."))))


 #_(.setEventListener mt4-conv-bid (DDEClientEventListener.))



