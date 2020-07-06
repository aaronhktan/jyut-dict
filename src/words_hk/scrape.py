import csv
from selenium import webdriver
import sys
import time

def read_csv(filename):
  with open(filename) as csvfile:
    reader = csv.reader(csvfile, delimiter=',')
    for row in reader:
      yield row[0]

index = 0

for item in read_csv(sys.argv[1]):
  index += 1
  if (index < int(sys.argv[2])):
    continue 

  options = webdriver.ChromeOptions();
  options.add_argument("--user-data-dir=selenium")

  driver = webdriver.Chrome(options=options)
  driver.get(f"https://words.hk/zidin/{item}")

  with open(f"./scraped/{item}.html", "w") as f:
    f.write(driver.page_source)

  driver.quit()

  time.sleep(10)
