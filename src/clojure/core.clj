(ns clj-dde.core
  [:import
        [com.pretty_tools.dde.client DDEClientConversation]
        [java.util Date]])

(System/getProperty "java.library.path")

(Date.)

(def conversation (DDEClientConversation.))

(.connect conversation "excel" "sheet1")

; #(com.pretty_tools.dde.client.DDEClientConversation. %)

; (.. conversation connect setTimeout 2000)
; (.. conversation connect "Excel" "Sheet1")


#_(defn get-a-conversation
  []
  (DDEClientConversation. com.pretty_tools.dde.client.DDEClientConversation))


