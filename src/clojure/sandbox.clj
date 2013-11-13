(.disconnect conversation)

(.execute conversation "some-string")

(.execute conversation some-byte)

(.poke conversation "some-item" "some-data")

(.poke conversation "some-item" byte-data some-clipboardformat)

(.request conversation "some-item")

(.startAdvice conversation "some-item")

(.stopAdvice conversation "some-item")

(.getService conversation)

(.getTopic conversation)

(.getTimeout conversation)

(.setTimeout conversation)

(.getEventListener conversation)

(.setEventListener conversation an-event-listener)

(.checkConversation conversation)

(.fireEventOnDisconnect conversation)

(.fireEventOnAdvData conversation)


; #(com.pretty_tools.dde.client.DDEClientConversation. %)

; (.. conversation connect setTimeout 2000)
; (.. conversation connect "Excel" "Sheet1")


#_(defn get-a-conversation
  []
  (DDEClientConversation. com.pretty_tools.dde.client.DDEClientConversation))
