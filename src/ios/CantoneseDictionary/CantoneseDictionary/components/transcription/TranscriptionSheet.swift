//
//  TranscriptionSheet.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-04.
//

import SwiftUI

struct TranscriptionView: View {
    @Environment(\.dismiss) var dismiss

    var body: some View {
        NavigationStack {
            Text("Transcription animation here")
                .toolbar {
                    ToolbarItem(placement: .topBarTrailing) {
                        Button("Close transcription", systemImage: "xmark") {
                            dismiss()
                        }
                        .labelsHidden()
                    }
                }
        }

    }
}

#Preview {
    TranscriptionView()
}
