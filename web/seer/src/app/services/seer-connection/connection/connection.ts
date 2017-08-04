// https://gearheart.io/blog/auto-websocket-reconnection-with-rxjs/
import { Observable, Observer, Subject } from 'rxjs';
import { WebSocketSubject, WebSocketSubjectConfig } from 'rxjs/observable/dom/WebSocketSubject';

// we inherit from the ordinary Subject
export class RxWebsocketSubject<T> extends Subject<T> {
    public readonly connectionStatus: Observable<boolean>;
    private reconnectionObservable: Observable<number>;
    private wsSubjectConfig: WebSocketSubjectConfig;
    private socket: WebSocketSubject<any>;
    private connectionObserver: Observer<boolean>;

    constructor(
        private readonly url: string,
        private readonly reconnectInterval: number = 5000,
        private readonly reconnectAttempts: number = Number.MAX_SAFE_INTEGER,
        private readonly resultSelector: (e: MessageEvent) => any = (e: MessageEvent): any => JSON.parse(e.data),
        private readonly serializer: (data: any) => string = (data: any): string => JSON.stringify(data)
    ) {
        super();

        // connection status
        this.connectionStatus = new Observable<boolean>((observer) => {
            this.connectionObserver = observer;
        }).share().distinctUntilChanged();

        // config for WebSocketSubject
        // except the url, here is closeObserver and openObserver to update connection status
        this.wsSubjectConfig = {
            url,
            closeObserver: {
                next: (e: CloseEvent) => {
                    this.socket = null;
                    this.connectionObserver.next(false);
                }
            },
            openObserver: {
                next: (e: Event) => {
                    this.connectionObserver.next(true);
                }
            }
        };
        // we connect
        this.connect();
        // we follow the connection status and run the reconnect while losing the connection
        this.connectionStatus.subscribe((isConnected) => {
            if (!this.reconnectionObservable && typeof (isConnected) === 'boolean' && !isConnected) {
                this.reconnect();
            }
        });
    }

    // sending the message
    public send(data: any): void {
        this.socket.next(this.serializer(data));
    }

    private connect(): void {
        this.socket = new WebSocketSubject(this.wsSubjectConfig);
        this.socket.subscribe(
            (m) => {
                this.next(m); // when receiving a message, we just send it to our Subject
            },
            (error: Event) => {
                if (!this.socket) {
                    // in case of an error with a loss of connection, we restore it
                    this.reconnect();
                }
            });
    }

    // reconnection
    private reconnect(): void {
        this.reconnectionObservable = Observable.interval(this.reconnectInterval)
            .takeWhile((v, index) => {
                return index < this.reconnectAttempts && !this.socket;
            });
        this.reconnectionObservable.subscribe(
            () => {
                this.connect();
            },
            null,
            () => {
                // if the reconnection attempts are failed, then we call complete of our Subject and status
                this.reconnectionObservable = null;
                if (!this.socket) {
                    this.complete();
                    this.connectionObserver.complete();
                }
            });
    }
}
