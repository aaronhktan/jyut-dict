//
//  SavedView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct SavedView: View {
  @Environment(\.dismiss) var dismiss

  var body: some View {
    NavigationStack {
      Text("Saved words here")
        .toolbar {
          #if os(iOS)
            ToolbarItem(placement: .topBarTrailing) {
              Button("Close saved words", systemImage: "xmark") {
                dismiss()
              }
              .labelsHidden()
            }
          #else
            ToolbarItem(placement: .navigation) {
              Button("Close saved words", systemImage: "xmark") {
                dismiss()
              }
              .labelsHidden()
            }
          #endif
        }
    }

  }
}

#Preview {
  SavedView()
}
