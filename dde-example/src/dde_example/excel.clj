(ns dde-example.excel
  (:require [clj-dde.core :refer :all]))



  (def excel-conv (conversation))
  (type excel-conv) ; check that a 'DDEClientConversation' instance has been created.

  (setTimeout excel-conv 2000)

  (connect excel-conv "excel" "sheet1")

  (request excel-conv "R1C1")

  (poke excel conv "R1C1" "new val")

