//
//  HandwritingView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct HandwritingView: View {
    @Environment(\.dismiss) var dismiss

    var body: some View {
        NavigationStack {
            Text("Handwriting interface here")
                .toolbar {
                    ToolbarItem(placement: .topBarTrailing) {
                        Button("Close handwriting view", systemImage: "xmark") {
                            dismiss()
                        }
                        .labelsHidden()
                    }
                }
        }

    }
}

#Preview {
    HandwritingView()
}
