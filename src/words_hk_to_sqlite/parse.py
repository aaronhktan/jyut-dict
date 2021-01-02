from bs4 import BeautifulSoup

from collections import defaultdict, namedtuple
import logging
import os
import sys

ExampleTuple = namedtuple('ExampleTuple', ['lang', 'pron', 'content'])
DefinitionTuple = namedtuple('DefinitionTuple', ['lang', 'content'])
MeaningTuple = namedtuple('Meaning', ['definitions', 'examplephrases', 'examplesentences'])
WordTuple = namedtuple('Word', ['word', 'pronunciation', 'meanings'])

def parse_file(file_name, words):
  with open(file_name, 'r') as file:
    soup = BeautifulSoup(file, 'html.parser')
    drafts = soup.find_all('div', class_='draft-version')

    # Each separate entry for a particular word is contained in a div with a class called "draft-version"
    for draft in drafts:
      # The word is the first h1 element, contained in the traditional Chinese quotes
      word = draft.find('h1').text
      word = word[word.find('「')+1:word.find('」')]

      word_pronunciation = draft.find('span', class_='zi-pronunciation').text

      meanings = []
      # This will find the table row containing meaning if there are multiple meanings
      # If there are multiple meanings, they will be in an ordered list, so extract every item in the list.
      try:
        list_items = draft.find('tr', class_='zidin-explanation').find_all('li')
        # This will find the table row containing meanings if there is a single meaning
        if not list_items:
          list_items = [draft.find('tr', class_='zidin-explanation').find_all('td')[1]]
      except:
        # If there is no zidin-explanation class, it might be an old page
        # These do not contain good formatting, so just stick it into the list and call it a day
        try:
          text = draft.find('li', class_=None).text
          definition = DefinitionTuple('yue', text) # Assume definition is in Cantonese
          meanings.append(MeaningTuple(definition, [], []))
          words[word].append(WordTuple(word, word_pronunciation, meanings))
          continue
        except:
          # Malformed page, give up
          logging.warning(f'Failed to parse for item {word}')
          continue

      logging.info(f'Parsing item {word}')
      for list_item in list_items:
        meaning = MeaningTuple([], [], [])

        # Each definition for one meaning is contained in a classless div
        def_divs = list_item.find_all('div', class_=None, recursive=False)
        for def_div in def_divs:
          try:
            # Extract the language of this definition, and then remove it from the tree
            lang_tag = def_div.find('span', class_='zi-item-lang', recursive=False)
            lang = lang_tag.text.strip('()')
            lang_tag.decompose()
          except:
            lang = 'yue' # Assume Cantonese definition if no lang tag found

          # Get the textual content of the definition
          content = def_div.text.strip()

          meaning.definitions.append(DefinitionTuple(lang, content))

        # Each example phrase for the definition is contained in a div with class zi-details-phrase-item
        exphr_elems = list_item.find_all('div', class_='zi-details-phrase-item', recursive=False)
        for exphr_elem in exphr_elems:
          exphr = []
          exphr_divs = exphr_elem.find_all('div', class_=None, recursive=False)
          for exphr_div in exphr_divs:
            lang_tag = exphr_div.find('span', class_='zi-item-lang')
            lang = lang_tag.text.strip('()')
            lang_tag.decompose()

            pronunciation_tag = exphr_div.find('span', class_='zi-item-phrase-pronunciation')
            if pronunciation_tag:
              pronunciation = pronunciation_tag.text.strip('()')
              pronunciation_tag.decompose()
            else:
              pronunciation = ''

            content = exphr_div.text.strip()
            exphr.append(ExampleTuple(lang, pronunciation, content))
          meaning.examplephrases.append(exphr)

        # Each example sentence for the definition is contained in a div with class zi-details-example-item
        exsen_elems = list_item.find_all('div', class_='zi-details-example-item', recursive=False)
        for exsen_elems in exsen_elems:
          exsen = []
          exsen_divs = exsen_elems.find_all('div', class_=None, recursive=False)
          for exsen_div in exsen_divs:
            lang_tag = exsen_div.find('span', class_='zi-item-lang')
            lang = lang_tag.text.strip('()')
            lang_tag.decompose()

            pronunciation_tag = exsen_div.find('span', class_='zi-item-phrase-pronunciation')
            if pronunciation_tag:
              pronunciation = pronunciation_tag.text.strip('()')
              pronunciation_tag.decompose()
            else:
              pronunciation = ''

            content = exsen_div.text.strip()
            exsen.append(ExampleTuple(lang, pronunciation, content))
          meaning.examplesentences.append(exsen)

        meanings.append(meaning)

      logging.debug(word, word_pronunciation)
      for index, meaning in enumerate(meanings):
        logging.debug(f'Meaning {index}:')
        for definition in meaning.definitions:
          logging.debug('\tDefinition: ', definition.lang, definition.content)
        for i, phrases in enumerate(meaning.examplephrases):
          logging.debug(f'\t\tPhrase {i}:')
          for phrase in phrases:
            logging.debug('\t\t\tPhrase: ', phrase.lang, phrase.content, phrase.pron)
        for i, sentences in enumerate(meaning.examplesentences):
          logging.debug(f'\t\tSentence {i}:')
          for sentence in sentences:
            logging.debug('\t\t\tSentence: ', sentence.lang, sentence.content, sentence.pron)
      words[word].append(WordTuple(word, word_pronunciation, meanings))

def parse_folder(folder_name, words):
  for index, entry in enumerate(os.scandir(folder_name)):
    if not index % 100:
      print(f'Parsed word #{index}')
    if entry.is_file() and entry.path.endswith('.html'):
      parse_file(entry.path, words)

if __name__ == '__main__':
  # logging.basicConfig(level='INFO')
  parsed_words = defaultdict(list)
  parse_folder(sys.argv[1], parsed_words)

