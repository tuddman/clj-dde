(ns dde-example.core
  (:require [clj-dde :refer :all]))

 #_(use 'clj-dde.core)

  (def mt4 (conversation))
  (type mt4)

  (setTimeout mt4 2000)

  (connect mt4 "MT4" "BID")


