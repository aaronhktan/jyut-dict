//
//  HistoryView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct HistoryView: View {
    @Environment(\.dismiss) var dismiss

    var body: some View {
        NavigationStack {
            Text("Search history here")
                .toolbar {
                    ToolbarItem(placement: .topBarTrailing) {
                        Button("Close history", systemImage: "xmark") {
                            dismiss()
                        }
                        .labelsHidden()
                    }
                }
        }

    }
}

#Preview {
    HistoryView()
}
