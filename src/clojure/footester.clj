(ns clj-dde.footester
  [:import
        [com.Baz.Foo]
        [java.util Date]])


;; ** Java Interop Syntax **

; (.instanceMember instance args*)
; (.instanceMember Classname args*)
; (Classname/staticMethod args*)
; Classname/staticField

(System/getProperty "java.library.path")

(Date.)

(Foo.)

(doto (new Foo) (.BazFn))

(doto (new Foo) (.connect "excel" "sheet1"))

(def conversation(new Foo))

(.connect conversation "excel" "sheet1")

; (defn new-instance [obj] (let [clazz (class obj)] (eval `(new ~clazz))))

; #_(def myFoo (new-instance com.Baz.Foo))

(defn fooas-fn
  []
  (str com.Baz.Foo/value))

(fooas-fn)

(defn foo
  []
  (println (com.Baz.Foo/value)))

(foo)
