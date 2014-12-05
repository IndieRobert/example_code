;
; Conquest of Wilderness
; Copyright (c) 2014 Frederic Brachfogel
; All rights reserved.
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions are met:
;     * Redistributions of source code must retain the above copyright
;       notice, this list of conditions and the following disclaimer.
;     * Redistributions in binary form must reproduce the above copyright
;       notice, this list of conditions and the following disclaimer in the
;       documentation and/or other materials provided with the distribution.
;     * The name of Frederic Brachfogel may not be used to endorse or promote products
;       derived from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY FREDERIC BRACHFOGEL ``AS IS'' AND ANY
; EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
; WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
; DISCLAIMED. IN NO EVENT SHALL FREDERIC BRACHFOGEL BE LIABLE FOR ANY
; DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
; (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
; ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
; SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;
; https://crackstation.net/hashing-security.htm
;
(ns lobby.core                                                                                                                                       
  (:import (org.eclipse.jetty.servlet ServletContextHandler ServletHolder)
           (org.eclipse.jetty.server.handler.ContextHandlerCollection))
  (:require [compojure.route :as route]))
(use 'ring.adapter.jetty
     'ring.middleware.stacktrace
     'ring.middleware.session
     'ring.util.servlet
     'ring.util.response
     'compojure.core
     '[ring.middleware.json :only [wrap-json-response]]
     '[clojure.java.shell :only [sh]])
(require '[crypto.password.pbkdf2 :as password])
;
;
(defonce release? true)
(defonce version  "0.0.1")
;
;
(defonce db-path "db/")
(defonce user-path (str db-path "user/"))
;
;
(defonce game-servers (atom (list)))
(defonce game-server (atom "162.243.200.154"))
(defonce game-server-port (atom 40000))
(defonce sessions (atom {}))
(defonce waiting_players_1v0 (atom (list)))
(defonce matched_players_1v0 (atom (list)))
(defonce matcher_1v0 (agent nil))
(defonce waiting_players_1v1 (atom (list)))
(defonce matched_players_1v1 (atom (list)))
(defonce matcher_1v1 (agent nil))
;
;
(defn has-finished? [p]
  (try 
    (.exitValue p)
    true
    (catch Exception e false)))
;
;
(defn set-interval [f ms]
  (future (while (f) (Thread/sleep ms))))
;
;
(defn user-exists? [username]
  (loop [fs (file-seq (clojure.java.io/file user-path))]
    (when (not (empty? fs))
     (if (= username (.getName (first fs)))
       true
       (recur (rest fs))))))
;
;
(defn get-user [username]
  (load-file (str user-path username)))
;
;
(defn user-ok? [username password]
  (password/check password ((get-user username) :password)))
;
;
(defn user-session-ok? [username token]
  (when (not (nil? (@sessions username)))
    (= token (.toString (@sessions username)))))
;
;
(defn parse-int-bis [s]
   (BigInteger. (re-find  #"\d+" s )))
;
;
(defn add-time [username timespent]
  (let [user (get-user username)]
    (spit (str user-path username) 
          (pr-str (assoc user :timespent 
                    (+ (user :timespent) 
                       timespent))))))
;
;
(defn clear-user-game [username]
  (swap! waiting_players_1v0 (fn [l] (filter #(not= username (%1 :username)) l)))
  (swap! waiting_players_1v1 (fn [l] (filter #(not= username (%1 :username)) l)))
  (swap! matched_players_1v0 (fn [l] (filter #(not= username (%1 :username)) l)))
  (swap! matched_players_1v1 (fn [l] (filter #(not= username (%1 :username)) l))))
;
;
(defroutes handler
  (GET "/register/:username/:password" {{username :username password :password} :params}
    (if (user-exists? username)
      (response {:already-exist? true})
      (let [user-db {:username username :password (password/encrypt password) :timespent 0}
            f       (str user-path username)
            token   (java.util.UUID/randomUUID)]
        (swap! sessions (fn [x] (assoc x username token)))
        (spit f (pr-str user-db))
        (response {:success true :token token}))))
  (GET "/login/:username/:password" {{username :username password :password} :params}
    (if (user-exists? username)
      (if (user-ok? username password)
        (let [token   (java.util.UUID/randomUUID)]
          (println (str username " is logged in."))
          (swap! sessions (fn [x] (assoc x username token)))
          (response {:success true :token token}))
        (response {:success false}))
      (response {:success false})))
  (GET "/logoff/:username/:token" {{username :username token :token} :params}
    (if (user-exists? username)
      (if (user-session-ok? username token)
        (do
          (println (str username " is logging off."))
          (clear-user-game username)
          (swap! sessions (fn [x] (dissoc x username)))
          (response {:success true}))
        (response {:success false}))
      (response {:success false})))
  (GET "/play1v1/:username/:token" {{username :username token :token} :params}
    (if (user-exists? username)
      (if (user-session-ok? username token)
        (do
          (clear-user-game username)
          (swap! waiting_players_1v1 (fn [x] (cons {:username username :token token} x)))
          (response {:success true}))
        (response {:need-login true}))
      (response {:need-register true})))
  (GET "/wait1v1/:username/:token/:foo" {{username :username token :token foo :foo} :params}
    (if (user-exists? username)
      (if (user-session-ok? username token)
        (let [matched-p (filter #(and (= username (%1 :username)) 
                                      (= token (%1 :token))) 
                                 @matched_players_1v1)]
          (if (not (empty? matched-p))
            (let [matched-p (first matched-p)]
              (swap! matched_players_1v0 (fn [l] (filter #(not= username (%1 :username)) l)))
              (swap! matched_players_1v1 (fn [l] (filter #(not= username (%1 :username)) l)))
              (response {:success           true 
                         :game-name         (matched-p :game-name)
                         :game-server   		(matched-p :game-server)
                         :game-server-port  (matched-p :game-server-port)
                         :player-name       username
                         :team-count        (matched-p :team-count)
                         :player-count      (matched-p :player-count)
                         :team-id           (matched-p :team-id)
                         :offline-players   (matched-p :offline-players)}))
            (response {:success false})))
        (response {:need-login true}))
      (response {:need-register true})))
  (GET "/cancel/:username/:token" {{username :username token :token} :params}
    (if (user-exists? username)
      (if (user-session-ok? username token)
        (do
          (swap! waiting_players_1v0 
            (fn [x] 
              (filter #(or (not= username (%1 :username)) 
                           (not= token (%1 :token))) 
                       x)))
          (swap! waiting_players_1v1 
            (fn [x] 
              (filter #(or (not= username (%1 :username)) 
                           (not= token (%1 :token))) 
                       x)))
          (response {:success false}))
        (response {:need-login true}))
      (response {:need-register true})))
  (GET "/playerconnected" []
    (response {:playerconnected (count @sessions)}))
  (GET "/version" []
    (response {:version version}))
  (GET "/timespent/:username/:token/:timespent" {{username :username token :token timespent :timespent} :params}
    (if (user-exists? username)
      (if (user-session-ok? username token)
        (do
          (add-time username (parse-int-bis timespent))
          (response {:success true}))
        (response {:need-login true}))
      (response {:need-register true}))))
;
;
(def app
  (-> #'handler
      (wrap-stacktrace)
      (wrap-json-response)
      (wrap-stacktrace)))
;
;
(defonce server (atom (run-jetty #'app
  (if release?
     {:configurator (fn [server]
                      (doseq [c (.getConnectors server)]
                        (when-not (or (nil? c) 
                                      (instance? org.eclipse.jetty.server.ssl.SslSocketConnector c))
                          (.removeConnector server c)))
                      server)
      :join? false
      :ssl? true
      :ssl-port 8080
      :keystore "keystore.jks"
      :key-password (load-file "key.pass")}
    {:join? false :port 8080}))))
(.stop @server)
;
;
(defn match_them1v1 [_]
  (loop []
    (when (not (.isStopped @server))
      (swap! 
        waiting_players_1v1 
        (fn [ps]
          (loop [l ps]
            (if (> (count l) 1)
              (let [game-name       (java.util.UUID/randomUUID)
                    team-count      3
                    player-count    4
                    offline-teams 	1
                    offline-players 3
                    p1            (first l)
                    new-p1        (assoc p1 
                                   :game-name         game-name
                                   :game-server  			@game-server
                                   :game-server-port  @game-server-port
                                   :player-count      player-count
                                   :team-count        team-count
                                   :offline-players 	offline-players
                                   :team-id           0)
                    p2            (second l)
                    new-p2        (assoc p2 
                                   :game-name         game-name
                                   :game-server  			@game-server
                                   :game-server-port  @game-server-port
                                   :player-count      player-count
                                   :team-count        team-count
                                   :offline-players 	offline-players
                                   :team-id           1)]
                (println (str "Matching " (new-p1 :username) " with " (new-p2 :username) " for game " game-name "."))
                (swap! matched_players_1v1 (fn [x] (cons new-p1 (cons new-p2 x))))
                (swap! game-servers (fn [x] (cons 
                                              (.. Runtime (getRuntime) 
                                                (exec (str "python gameserver.py " @game-server-port " \"" game-name "\" " team-count " " player-count " " offline-teams " " offline-players)))
                                              x)))
                (set-interval 
                  (fn [] 
                    (let [finished? (has-finished? (first @game-servers))]
                      (when finished?
                        (println 
                          (slurp 
                            (str 
                              "./logs/log" 
                              (clojure.string/replace 
                                (.toString game-name)
                                #"-" 
                                "_")))))
                      (not finished?)))
                  250)
                (swap! game-server-port #(+ 32 %))
                (recur (drop 2 l)))
              l))))
      (recur))))
;
;
(defn start []
  (.start @server)
  (send matcher_1v1 match_them1v1))
;
;
(defn stop []
  (doseq [game-server @game-servers]
    (.destroy game-server))
  (.stop @server)
  (swap! game-servers (fn [_] (list)))
  (swap! game-server-port (fn [_] 40000))
  (swap! waiting_players_1v0 (fn [_] (list)))
  (swap! matched_players_1v0 (fn [_] (list)))
  (send matcher_1v0 (fn [_] nil))
  (swap! waiting_players_1v1 (fn [_] (list)))
  (swap! matched_players_1v1 (fn [_] (list)))
  (send matcher_1v1 (fn [_] nil))
  ;(swap! sessions (fn [_] {}))
  "Good bye.")
;
;
(defn -main [& args])
