//
//  ContentView.swift
//  Test
//
//  Created by Aaron on 2026-09-01.
//

import SwiftUI

struct ContentView: View {
    @State private var searchText: String = ""
    @State private var showTranscription: Bool = false

    var body: some View {
        NavigationStack {
            SearchingView(searchText: $searchText)
                .toolbar {
                    ToolbarItem(placement: .topBarTrailing) {
                        ControlGroup {
                            Button(
                                action: { print("clicked history") }
                            ) {
                                Image(systemName: "clock")
                            }
                            Button(
                                action: { print("clicked saved") }
                            ) {
                                Image(systemName: "star")
                            }
                            Button(
                                action: { print("clicked settings") }
                            ) {
                                Image(systemName: "gearshape")
                            }
                        }
                    }
                    DefaultToolbarItem(kind: .search, placement: .bottomBar)
                    ToolbarSpacer(.fixed, placement: .bottomBar)
                    ToolbarItem(placement: .bottomBar) {
                        ControlGroup {
                            Button(action: {
                                showTranscription = true
                            }) {
                                Image(systemName: "microphone")
                            }
                            .sheet(isPresented: $showTranscription) {
                                TranscriptionView()
                                    .presentationDetents([.medium])
                            }

                            Button(action: {
                                print("clicked pencil")
                            }
                            ) {
                                Image(systemName: "pencil.and.scribble")
                            }
                        }
                    }
                }
                .searchable(
                    text: $searchText,
                    prompt: "Search"
                )
                .searchDictationBehavior(.inline(activation: .onSelect))
        }
    }
}

struct SearchingView: View {
    @Environment(\.isSearching) private var isSearching
    @Binding var searchText: String

    var options: [String] = [
        "Auto-detect language", "Traditional Chinese", "Simplified Chinese",
        "Fuzzy Jyutping", "Jyutping", "Fuzzy Pinyin", "Pinyin", "English",
    ]
    @State private var selectedOption: String = "Auto-detect language"

    var body: some View {
        if isSearching {
            ZStack(alignment: .bottom) {
                List(
                    "0 1 2 3 4 5 6 7 8 9 a b c d e f g h i j k l m n o p q r s t u v w x y z"
                        .components(
                            separatedBy: " "
                        ),
                    id: \.self
                ) { word in
                    if searchText == "" {
                        Text(word)
                    } else if word.lowercased().contains(
                        searchText.lowercased()
                    ) {
                        Text(word)
                    }
                }
                .listStyle(.plain)

                Picker("Search Options", selection: $selectedOption) {
                    ForEach(options, id: \.self) { option in
                        Text(option)
                    }
                }
                .glassEffect()
            }
        } else {
            ContentUnavailableView {
                Label {
                    Text("Welcome to Jyut Dictionary")
                } icon: {
                    Image("AppIconImage")
                        .resizable()
                        .scaledToFit()
                        .frame(width: 200)
                }
                Text("A Project by Aaron Tan")
                    .padding(.top, 10)
                    .foregroundStyle(Color.secondary)
            }
        }
    }
}

#Preview {
    ContentView()
}
