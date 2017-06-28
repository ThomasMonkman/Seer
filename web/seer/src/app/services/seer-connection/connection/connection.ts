import { QueueingSubject } from 'queueing-subject'
import { Observable } from 'rxjs/Rx';
import websocketConnect from 'rxjs-websockets'

export class Connection {
    public readonly url: string;
    public readonly inputStream: QueueingSubject<any>;
    public readonly messages: Observable<any>;
    constructor(url) {
        this.url = url;
        this.inputStream = new QueueingSubject<any>();
        const arghh = websocketConnect(this.url, this.inputStream).messages;
        this.messages = arghh;
    }

    public send(message: any): void {
        this.inputStream.next(message);
    }
}