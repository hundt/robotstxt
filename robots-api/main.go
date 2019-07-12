package main

import (
	"bufio"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net/http"
	"os"
	"os/exec"
	"strconv"
	"strings"
)

func main() {
	http.HandleFunc("/check", func(w http.ResponseWriter, r *http.Request) {
		robotsUpload, _, err := r.FormFile("robots")
		if err != nil {
			http.Error(w, fmt.Sprintf("Error reading robots file: %s", err), http.StatusBadRequest)
			return
		}
		robotsFile, err := ioutil.TempFile(os.TempDir(), "check-")
		if err != nil {
			http.Error(w, fmt.Sprintf("Error writing out robots file: %s", err), http.StatusInternalServerError)
			return
		}
		_, err = io.Copy(robotsFile, robotsUpload)
		if err != nil {
			http.Error(w, fmt.Sprintf("Error writing out robots file: %s", err), http.StatusInternalServerError)
			return
		}
		robotsFile.Sync()

		cmd := exec.Command("check_robots", robotsFile.Name())
		out, err := cmd.CombinedOutput()
		if err != nil {
			http.Error(w, fmt.Sprintf("Error processing robots file: %s: %s", err, out), http.StatusInternalServerError)
			return
		}
		updates := make(map[int]string)
		for _, line := range strings.Split(string(out), "\n") {
			line = strings.TrimSpace(line)
			if line != "" {
				pieces := strings.SplitN(line, " ", 2)
				n, err := strconv.Atoi(pieces[0])
				if err != nil {
					http.Error(w, fmt.Sprintf("Error processing robots file: %s", err), http.StatusInternalServerError)
					return
				}
				updates[n] = pieces[1]
			}
		}

		robotsFile.Seek(0, 0)
		lineNum := 0
		scanner := bufio.NewScanner(robotsFile)
		for scanner.Scan() {
			lineNum += 1
			if update, ok := updates[lineNum]; ok {
				fmt.Fprintf(w, "%s\n", update)
			} else {
				fmt.Fprintf(w, "%s\n", scanner.Bytes())
			}
		}
	})

	http.HandleFunc("/urls", func(w http.ResponseWriter, r *http.Request) {
		robotsUpload, _, err := r.FormFile("robots")
		if err != nil {
			http.Error(w, fmt.Sprintf("Error reading robots file: %s", err), http.StatusBadRequest)
			return
		}
		robotsFile, err := ioutil.TempFile(os.TempDir(), "urls-")
		if err != nil {
			http.Error(w, fmt.Sprintf("Error writing out robots file: %s", err), http.StatusInternalServerError)
		}
		_, err = io.Copy(robotsFile, robotsUpload)
		if err != nil {
			http.Error(w, fmt.Sprintf("Error writing out robots file: %s", err), http.StatusInternalServerError)
			return
		}
		robotsFile.Close()

		urlsUpload, _, err := r.FormFile("urls")
		if err != nil {
			http.Error(w, fmt.Sprintf("Error reading urls file: %s", err), http.StatusBadRequest)
			return
		}
		urlsFile, err := ioutil.TempFile(os.TempDir(), "urls-")
		if err != nil {
			http.Error(w, fmt.Sprintf("Error writing out urls file: %s", err), http.StatusInternalServerError)
			return
		}
		_, err = io.Copy(urlsFile, urlsUpload)
		if err != nil {
			http.Error(w, fmt.Sprintf("Error writing out urls file: %s", err), http.StatusInternalServerError)
			return
		}
		urlsFile.Close()

		userAgent := r.FormValue("userAgent")

		cmd := exec.Command("check_urls", robotsFile.Name(), userAgent, urlsFile.Name())
		out, err := cmd.CombinedOutput()
		if err != nil {
			http.Error(w, fmt.Sprintf("Error checking urls: %s: %s", err, out), http.StatusInternalServerError)
			return
		}

		fmt.Fprintf(w, "%s", out)
	})

	http.HandleFunc("/health", func(w http.ResponseWriter, r *http.Request) {})

	listenAddr := ":80"
	log.Printf("Listen on %s", listenAddr)
	log.Fatal(http.ListenAndServe(listenAddr, nil))
}
