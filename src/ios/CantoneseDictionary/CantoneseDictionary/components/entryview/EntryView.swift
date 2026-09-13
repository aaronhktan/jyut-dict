//
//  EntryView.swift
//  CantoneseDictionary
//
//  Created by Aaron on 2026-09-12.
//

import SwiftUI

struct EntryView: View {
    let title: String

    var body: some View {
        Text("Selected text: \(title)")
            .font(.largeTitle)
    }
}
