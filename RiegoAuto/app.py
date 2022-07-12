
from kivy.app import App
from kivy.uix.label import Label


class Main(App):
    def build(self):
        return Label(text='Hola SAE el Riego comenzara...')
        


Main().run()