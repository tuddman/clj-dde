(ns clj-dde.core
  [:import
        [com.pretty_tools.dde.client.DDEClientConversation]
        [com.pretty_tools.dde.client.DDEClientEventListener]
        [com.pretty_tools.dde ClipboardFormat]
        [com.pretty_tools.dde DDEException]
        [com.pretty_tools.dde DDEMLException]
        [java.util Date]])

(System/getProperty "java.library.path")

(Date.)


;; instantiate a DDEClientConversation

;; ** DDEClientConversation member functions:

; (.connect conversation "excel" "sheet1")
; (.request conversation "R1C1")
; (.poke conversation "R1C1", "new val")

(def mt4-conv (DDEClientConversation.))

(.connect mt4-conv "MT4" "BID")

(.startAdvice mt4-conv "EURUSD")

; (.setEventListener mt4-link (DDEClientEventListener))

;    void onItemChanged(String topic, String item, String data);

(defn ELOnItemChanged
  []
(reify DDEClientEventListener
  (onItemChanged [this topic item data]
               (str "onItemChanged: " topic  " , " item  " , " data " )"))))

(defn ELOnDisconnect
  []
(reify DDEClientEventListener
  (onDisconnect [this]
               (str "onDisconnect called."))))

(.setEventListener mt4-conv (ELOnItemChanged))

(ELOnItemChanged)


(str "the" "string" "gets")














