from bs4 import BeautifulSoup

from collections import namedtuple
import sys

ExampleTuple = namedtuple('ExampleTuple', ['lang', 'pron', 'content'])
DefinitionTuple = namedtuple('DefinitionTuple', ['lang', 'content'])
MeaningTuple = namedtuple('Meaning', ['definitions', 'examplephrases', 'examplesentences'])

with open(sys.argv[1], 'r') as file:
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
      # Malformed page, give up
      continue

    for list_item in list_items:
      meaning = MeaningTuple([], [], [])

      # Each definition for one meaning is contained in a classless div
      def_divs = list_item.find_all('div', class_=None, recursive=False)
      for def_div in def_divs:
        # Extract the language of this definition, and then remove it from the tree
        lang_tag = def_div.find('span', class_='zi-item-lang', recursive=False)
        lang = lang_tag.text.strip('()')
        lang_tag.decompose()

        # Get the textual content of the definition
        content = def_div.text.strip()

        meaning.definitions.append(DefinitionTuple(lang, content))

      # Each example phrase for the definition is contained in a div with class zi-details-phrase-item
      exphr_elems = list_item.find_all('div', class_='zi-details-phrase-item', recursive=False)
      for exphr_elem in exphr_elems:
        exphr_list = []
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
          exphr_list.append(ExampleTuple(lang, pronunciation, content))

        meaning.examplephrases.append(exphr_list)
        
      # Each example sentence for the definition is contained in a div with class zi-details-example-item
      exsen_elems = list_item.find_all('div', class_='zi-details-example-item', recursive=False)
      for exsen_elems in exsen_elems:
        exsen_list = []
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
          exsen_list.append(ExampleTuple(lang, pronunciation, content))

        meaning.examplesentences.append(exsen_list)

      meanings.append(meaning)

    print(word, word_pronunciation)
    for index, meaning in enumerate(meanings):
      print(index)
      for definition in meaning.definitions:
        print('\tDefinition: ', definition.lang, definition.content)
      for phrases in meaning.examplephrases:
        for phrase in phrases:
          print('\t\tPhrase: ', phrase.lang, phrase.content, phrase.pron)
      for sentences in meaning.examplesentences:
        for sentence in sentences:
          print('\t\tSentence: ', sentence.lang, sentence.content, sentence.pron)

