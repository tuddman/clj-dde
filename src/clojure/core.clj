(ns clj-dde.core
  [:import
            [com.pretty_tools.dde.client.DDEClientConversation]
            [main.java.Foo]
            [java.util Date]])

;;  DDEClientConversation DDEClientEventListener]

;; ** Java Interop Syntax **

; (.instanceMember instance args*)
; (.instanceMember Classname args*)
; (Classname/staticMethod args*)
; Classname/staticField

(System/getProperty "java.library.path")

(new com.pretty_tools.dde.client.DDEClientConversation DDEClientConversation)

(defn fooas-fn
  []
  (str main.java.Foo/value))

(defn foo
  []
  (println (main.java.Foo/value)))

; (import java.util.Date)
; (def *now* (Date.))

; (import com.prettty_tools.dde.client )

; (def DDECC (DDEClientConversation))

; (def conversation (com.pretty_tools.dde.client.DDEClientConversation))

#_(defn get-a-conversation
  []
  (DDEClientConversation. com.pretty_tools.dde.client.DDEClientConversation))


(fooas-fn)

(foo)
