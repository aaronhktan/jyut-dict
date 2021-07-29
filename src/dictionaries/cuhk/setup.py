from bs4 import BeautifulSoup
import requests

import sys

if len(sys.argv) != 2:
  print("Usage: python3 script.py output_filename.txt")
  sys.exit(1)

r = requests.get("https://apps.itsc.cuhk.edu.hk/hanyu/Page/Terms.aspx")
soup = BeautifulSoup(r.text, "html.parser")

# Extract links to each category
main_panel = soup.find("div", id="MainContent_panelTermsIndex")
main_table = main_panel.find("tbody").find("tr")
columns = main_table.find_all("td") # Yes, each column in the table is a <td>. Why? I don't know either.

category_links = []
for column in columns:
  rows = column.find_all("td")
  for row in rows:
    if row.find("a"):
      category_links.append("https://apps.itsc.cuhk.edu.hk/hanyu/Page/" + row.find("a").get("href"))

# Extract links for each word in each category
word_links = []
for category_link in category_links:
  print(f"Getting data for category {category_link}...")
  r = requests.get(category_link)
  soup = BeautifulSoup(r.text, "html.parser")

  main_table = soup.find("div", id="MainContent_panelTermsQuery").find("table")
  word_link_elems = main_table.find_all("a")
  for word_link_elem in word_link_elems:
    word_links.append(word_link_elem.get_text() + "\t" + "https://apps.itsc.cuhk.edu.hk/hanyu/Page/" + word_link_elem.get("href") + "\n")

# Write all links to file
with open(sys.argv[1], "w") as output_file:
  output_file.writelines(word_links)
