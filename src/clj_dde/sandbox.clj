(.disconnect conversation); implemented.

(.execute conversation "some-string")  ; implemented.

(.execute conversation some-byte)

(.poke conversation "some-item" "some-data"); implemented.

(.poke conversation "some-item" byte-data some-clipboardformat)

(.request conversation "some-item"); implemented.

(.startAdvice conversation "some-item"); implemented.

(.stopAdvice conversation "some-item") ; implemented.

(.getService conversation)

(.getTopic conversation)

(.getTimeout conversation)  ; implemented.

(.setTimeout conversation 2000) ; implemented.

(.getEventListener conversation)

(.setEventListener conversation an-event-listener) ; implemented.

; (.checkConversation conversation)

; (.fireEventOnDisconnect conversation)

; (.fireEventOnAdvData conversation)




