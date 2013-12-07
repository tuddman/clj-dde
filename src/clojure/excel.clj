(ns clj-dde.excel
;  (:gen-class)
  [:import
        [com.pretty_tools.dde.client DDEClientConversation]
        [com.pretty_tools.dde.client DDEClientEventListener]
        [com.pretty_tools.dde ClipboardFormat]
        [com.pretty_tools.dde DDEException]
        [com.pretty_tools.dde DDEMLException]])



(def excel-conv (DDEClientConversation.))
(.connect excel-conv "excel" "sheet1")
(.request excel-conv "R1C1")
(.startAdvice excel-conv "R1C1")
(.poke  excel-conv "R1C1", "new val")
