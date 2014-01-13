(ns dde-example.metatrader4
  (:require [clj-dde.core :as dde]))


;; MetaTrader --

; *note* the 'Enable DDE Server' box must be checked in Metatrader -> Tools -> Options -> Server


  (def mt4 (dde/conversation))
  (type mt4)

  (dde/setTimeout mt4 2000)

  (dde/connect mt4 "MT4" "BID")

  (dde/startAdvice mt4 "EURUSD")

  (dde/request mt4 "EURUSD")

  (dde/listen-and-print-data mt4)

  (dde/getService mt4)

  (dde/getTopic mt4)

  (stopAdvice mt4 "EURUSD")

  (disconnect mt4)








