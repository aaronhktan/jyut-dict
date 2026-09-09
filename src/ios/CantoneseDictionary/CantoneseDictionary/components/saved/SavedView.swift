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
                    ToolbarItem(placement: .topBarTrailing) {
                        Button("Close saved words", systemImage: "xmark") {
                            dismiss()
                        }
                        .labelsHidden()
                    }
                }
        }

    }
}

#Preview {
    SavedView()
}
