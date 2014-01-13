(ns dde-example.excel
  (:require
   [clj-dde.core :as dde]))


  (def excel-conv (dde/conversation))
  (type excel-conv) ; check that a 'DDEClientConversation' instance has been created.

  (dde/setTimeout excel-conv 2000)

  (dde/connect excel-conv "excel" "sheet1")

  (dde/request excel-conv "R1C1")

  (dde/poke excel-conv "R1C1" "I've been poked!")

  (dde/startAdvice excel-conv "R1C1")

  (dde/listen-and-print-data excel-conv)














