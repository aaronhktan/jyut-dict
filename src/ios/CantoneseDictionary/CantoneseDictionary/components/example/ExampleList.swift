//
//  ExampleList.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-29.
//

import SwiftUI
import os

struct ExampleList: View {

  let examples: [Example]

  var body: some View {
    List {
      ForEach(examples, id: \.id) { example in
        NavigationLink {
          ExampleDetail(example: example)
        } label: {
          ExampleRow(example: example)
        }
      }
    }
    .contentMargins(.top, 0)
    .navigationTitle("Examples")
  }
}
