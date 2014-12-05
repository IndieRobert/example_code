(ns prep.core
  (:use prep.gen :reload)
  (:use prep.unit_desc :reload)
  (:use prep.skill_desc :reload)
  (:use clojure.java.io))
;
;
(defn gen-all []
  (gen-unit)
  (gen-skill)
  "Have a nice day.")
;
;
(defn -main [& args]
  (gen-all))
