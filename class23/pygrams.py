#!/usr/bin/python3
import pdb
import sys

class bigrams:
    def __init__(self, file_path):
        self.file_path = file_path

    def read_txt(self, file_path):
        list_words = []
        with open(file_path, "r") as f:
            i = 0
            for line in f:
                words = line.split()
                for word in words:
                    if not word.isalpha():
                        list_word = list(word)
                        count = 0
                        for letter in word:
                            if not letter.isalpha():
                                list_word[count] = ' '
                            count += 1
                        word = "".join(list_word)
                        split_words = word.split()
                        for split_word in split_words:
                            list_words.append(split_word)                
                    else:
                        list_words.append(word)
                i += 1
        return list_words

    def create_bigram(self, list_words):
        bi_grams = []
        for i in range(len(list_words) - 1):
            word_1 = list_words[i]
            word_2 = list_words[i + 1]
            bi_gram = word_1 + ' ' + word_2
            bi_grams.append(bi_gram)
        return bi_grams

    def create_hash(self, bi_grams):
        hash_table = dict()
        hash_table_size = sys.getsizeof(hash_table)
        print(f"Hashtable started with {sys.getsizeof(hash_table)} bytes at length {len(hash_table)}")
        for bi_gram in bi_grams:
            hash_table[bi_gram] = hash_table.get(bi_gram, 0) + 1
            if (sys.getsizeof(hash_table) > hash_table_size):
                print(f"Hashtable grew to {sys.getsizeof(hash_table)} bytes at length {len(hash_table)}")
                hash_table_size = sys.getsizeof(hash_table)
        return hash_table

    def print_ans(self, hash_table):
        b_found_200 = False
        for key, value in hash_table.items():
            if value >= 200:
                print(f"Bigram '{key}' has count of {value}")
                b_found_200 = True

        if not b_found_200:
            for key, value in hash_table.items():
                print(f"Bigram '{key}' has count of {value}")

        print(f"Total of {len(hash_table)} different bigrams recorded")

if __name__ == "__main__":
    txt_file = "book.txt"
    pygrams = bigrams(txt_file)
    list_words = pygrams.read_txt(txt_file)
    bi_grams = pygrams.create_bigram(list_words)
    hash_table = pygrams.create_hash(bi_grams)
    pygrams.print_ans(hash_table)
    