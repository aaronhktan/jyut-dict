//
//  SearchHistoryRow.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-10-14.
//

import SwiftUI

struct SearchHistoryRow: View {
  let historyItem: SearchHistoryItem

  var body: some View {
    VStack {
      Text(
        historyItem.searchTerm
      )
      .font(.title)
      .rowLine()
      
      Text(
        historyItem.inputMethod.descriptionString
      )
      .font(.subheadline)
      .foregroundStyle(.secondary)
      .rowLine()
    }
  }
}

#Preview {
  let searchHistoryItem = SearchHistoryItem(
    searchTerm: "hello",
    inputMethod: .autoDetect
  )

  SearchHistoryRow(historyItem: searchHistoryItem)
}
