var gui=require('./gtk');
var sys=require('sys');

var wnd = gui.window({
	title: 'Тестовое приложение',
	width: 600,
	height: 400,
	opacity: 0.7,
	resizable: true,
	position: 1,
	items: [
		gui.button({
			title: 'шапка',
		}),
		gui.h([
			gui.button({
				title: 'лево',
			}),
			gui.button({
				title: 'право',
				onClick: function() {
					gui.alert('wow');
				}
			})
		]),
		gui.button({
			title: 'футер',
		})
	],
	on_close: function() {
		gui.alert('Good bie!');
	}
}).show();
