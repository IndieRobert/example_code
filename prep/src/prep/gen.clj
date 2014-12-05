(ns prep.gen
	(:use clojure.java.io))
;
;
(defn copy-file [source-path dest-path]
  (let [src (file source-path)]
    (when (.exists src)
      (copy src (file dest-path)))))
;
;
(defn save-cpp [s f]
  (when (not= s "")
    (let [f (clojure.string/replace f "gen-" "")
          f (clojure.string/replace f "-" "_")]
      (spit (str "../src/gen/" f ".cpp") s))))
;
;
(defn gen-on-desc [desc f]
  (apply 
      str 
      (map 
        (fn [x]
          (f x))
        desc)))
;
;
(def base-attack-time 1250)
(def base-hp 50)
(def base-damage 10)
(def base-mana-regen-time 10000)
;
;
(defn hp [x]
  (int (* x base-hp)))
;
;
(defn hp-regen [x]
  (* 100 (hp x)))
;
;
(defn damage [x]
  (int (* x base-damage)))
;
;
(def base-time-armor
  (* base-attack-time (/ base-hp base-damage)))
;
;
(defn hp-time [x]
  (int (* x base-time-armor)))
;
;
(defn mana-wait [x]
  (/ (* x base-mana-regen-time) base-time-armor))
;
;
(defn unit-type [x]
  (str "TR_unit_type_" x))