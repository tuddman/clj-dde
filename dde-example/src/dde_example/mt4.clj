(ns dde-example.mt4
  (:require [clj-dde.core :refer :all]))


;; MetaTrader --

; *note* the 'Enable DDE Server' box must be checked in Metatrader -> Tools -> Options -> Server


  (def mt4-bid (conversation))
  (type mt4-bid)

  (setTimeout mt4-bid 2000)

  (connect mt4-bid "MT4" "BID")

  (startAdvice mt4-bid "EURUSD")

  ;; request a 'one-time snapshot' request of an item
  (request mt4-bid "EURUSD")

  #_(setEventListener mt4)

  (listen mt4-bid)

  (listen-and-print-data mt4-bid)

  (getService mt4-bid)

  (getTopic mt4-bid)

  (stopAdvice mt4-bid "EURUSD")

  (disconnect mt4-bid)

















