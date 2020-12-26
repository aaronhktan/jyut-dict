from selenium import webdriver
import os
import requests
import sys

# Download the word list to location specified by first arg
wordslist_path = sys.argv[1]
if not os.path.exists(os.path.dirname(wordslist_path)):
  os.makedirs(os.path.dirname(wordslist_path))

wordslist = requests.get("https://words.hk/faiman/analysis/wordslist.csv")
with open(wordslist_path, "wb") as f:
  f.write(wordslist.content)

# Let the user log into words.hk so scraping words is possible
options = webdriver.ChromeOptions();
options.add_argument("--user-data-dir=selenium")

driver = webdriver.Chrome(options=options)
driver.get("https://words.hk")

