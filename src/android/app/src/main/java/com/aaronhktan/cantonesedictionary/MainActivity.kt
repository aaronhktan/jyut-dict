package com.aaronhktan.cantonesedictionary

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import com.aaronhktan.cantonesedictionary.logic.database.DatabaseManager
import com.aaronhktan.cantonesedictionary.ui.components.CantoneseDictionaryApp
import com.aaronhktan.cantonesedictionary.ui.theme.CantoneseDictionaryTheme

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        enableEdgeToEdge()
        setContent {
            CantoneseDictionaryTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    CantoneseDictionaryApp(
                        modifier = Modifier
                            .padding(innerPadding)
                    )
                }
            }
        }
    }
}


@Preview(showBackground = true)
@Composable
fun CantoneseDictionaryAppPreview() {
    CantoneseDictionaryTheme {
        Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
            CantoneseDictionaryApp(
                modifier = Modifier
                    .padding(innerPadding)
            )
        }
    }
}