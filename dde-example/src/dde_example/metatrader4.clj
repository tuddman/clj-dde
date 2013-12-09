(ns dde-example.metatrader4
  (:require [clj-dde.core :refer :all]))


;; MetaTrader --

; *note* the 'Enable DDE Server' box must be checked in Metatrader -> Tools -> Options -> Server


  (def mt4 (conversation))
  (type mt4)

  (setTimeout mt4 2000)

  (connect mt4 "MT4" "BID")

  (startAdvice mt4)
(request mt4 "EURUSD")


