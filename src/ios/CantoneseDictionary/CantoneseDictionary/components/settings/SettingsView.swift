//
//  SettingsView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-08.
//

import SwiftUI

struct SettingsView: View {
    @Environment(\.dismiss) var dismiss

    var body: some View {
        NavigationStack {
            Text("Settings screen")
                .toolbar {
                    ToolbarItem(placement: .topBarTrailing) {
                        Button("Close settings", systemImage: "xmark") {
                            dismiss()
                        }
                        .labelsHidden()
                    }
                }
                .navigationTitle("Settings")
        }

    }
}

#Preview {
    SettingsView()
}
