(ns clj-dde.core
  [:import
        [com.pretty_tools.dde.ClipboardFormat]
        [com.pretty_tools.dde.DDEException]
        [com.pretty_tools.dde.DDEMLException]
        [com.pretty_tools.dde.client DDEClientConversation]
        [main.java.Foo]
        [java.util Date]])

;;  DDEClientConversation DDEClientEventListener]

;; ** Java Interop Syntax **

; (.instanceMember instance args*)
; (.instanceMember Classname args*)
; (Classname/staticMethod args*)
; Classname/staticField

(System/getProperty "java.library.path")

(Date.)

#(com.pretty_tools.dde.client.DDEClientConversation. %)

(defn new-instance [obj] (let [clazz (class obj)] (eval `(new ~clazz))))

(def myFoo (new-instance main.java.Foo))

(def conversation (new-instance com.pretty_tools.dde.client.DDEClientConversation))

(.. conversation connect setTimeout 2000)
(.. conersation connect "Excel" "Sheet1")

; (new .DDEClientConversation com.pretty_tools.dde.client.DDEClientConversation)

(defn fooas-fn
  []
  (str main.java.Foo/value))

(fooas-fn)

(defn foo
  []
  (println (main.java.Foo/value)))

(foo)


; (import com.prettty_tools.dde.client )

; (def DDECC (DDEClientConversation))

; (def conversation (com.pretty_tools.dde.client.DDEClientConversation))

#_(defn get-a-conversation
  []
  (DDEClientConversation. com.pretty_tools.dde.client.DDEClientConversation))

